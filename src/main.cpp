#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include <math.h>

// for convenience
using json = nlohmann::json;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != std::string::npos) {
    return "";
  }
  else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main()
{
  uWS::Hub h;

  PID pid = PID();
  double params[] = {0.08, 0, 1.5};
    //0.08, 0, 1.5
  double deltas[] = {2, 2, 2};
  unsigned int on = 0;
  bool increasing = true;
  int count = 0;
  double total_cte = 0;
  double best_cte = 100000;
    double max_angle = 0;
  pid.Init(params); //10, 0.0000001, 0.001

  h.onMessage([&pid, &params, &deltas, &on, &increasing, &count, &total_cte, &best_cte, &max_angle](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
      bool twiddling = false;
    if (length && length > 2 && data[0] == '4' && data[1] == '2')
    {
      auto s = hasData(std::string(data));
      if (s != "") {
        auto j = json::parse(s);
        std::string event = j[0].get<std::string>();
        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<std::string>());
          double speed = std::stod(j[1]["speed"].get<std::string>());
          double angle = std::stod(j[1]["steering_angle"].get<std::string>());
          double steer_value;
          /*
          * TODO: Calcuate steering value here, remember the steering value is
          * [-1, 1].
          * NOTE: Feel free to play around with the throttle and speed. Maybe use
          * another PID controller to control the speed!
          */
          json msgJson;
          if (twiddling && count == 100000) {
            double this_cte = total_cte / count;
            if (this_cte < best_cte) {
              //deltas[on] /= 2;
              if (deltas[on] < 1) {
                  deltas[on] = 1.0000001;
              }
              best_cte = this_cte;
              on = (on + 1)%3;
              params[on] *= deltas[on];
              increasing = true;
            } else {
              if (increasing) {
                params[on] /= deltas[on]*deltas[on];
                increasing = false;
              } else {
                params[on] *= deltas[on];
               // deltas[on] *= 2;
                on = (on + 1)%3;
                params[on] *= deltas[on];
                increasing = true;
              }
            }
            std::cout << "params: " << params[0] << " " <<  params[1] << " " << params[2] <<  std::endl;
            std::cout << "deltas: " << deltas[0] << " " <<  deltas[1] << " " <<  deltas[2] << std::endl;
            total_cte = 0;
            count = 0;
            pid.Init(params);
            std::string msg = "42[\"reset\",{}]";
            ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
          }
          else {
            count++;
              std::cout << "mean for twiddle: " << total_cte/count << std::endl;
            steer_value = pid.UpdateError(cte);
              if (count > 100) {
                  total_cte += cte*cte;
              }
              //total_cte += std::abs(steer_value);
              if (steer_value < -1) {
                  if (std::abs(steer_value) > max_angle) {
                      max_angle = std::abs(steer_value);
                      std::cout << "max angle" <<  max_angle << std::endl;
                  }
                  steer_value = -1;
              } else if (steer_value > 1) {
                  if (std::abs(steer_value) > max_angle) {
                      max_angle = std::abs(steer_value);
                      std::cout << "max angle" <<  max_angle << std::endl;
                  }
                  steer_value = 1;
              }
            double throttle = -std::abs(cte) + 2.5 - speed/50; // taper throttle at high speeds/cte
            // DEBUG
            //std::cout << "CTE: " << cte << " Steering Value: " << steer_value << std::endl;

            msgJson["steering_angle"] = steer_value;
            msgJson["throttle"] = throttle;
            auto msg = "42[\"steer\"," + msgJson.dump() + "]";
            std::cout << msg << std::endl;
            ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
          }

        }
      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }
  });

  // We don't need this since we're not using HTTP but if it's removed the program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1)
    {
      res->end(s.data(), s.length());
    }
    else
    {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port))
  {
    std::cout << "Listening to port " << port << std::endl;
  }
  else
  {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  h.run();
}
