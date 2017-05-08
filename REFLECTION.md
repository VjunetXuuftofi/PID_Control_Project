# Reflection

In this project, I developed a PID controller capable of driving a car
through a simulator at fairly high speeds. Here's what it looked like:

## Final Video:

![Final Video](screencasts/final_video.gif "Final Video")

## Components


### Chosing components

I chose all of my components with a combination of a twiddle-like
algorithm and manual tuning. My original twiddle found it hard to quickly
home in on the very different scales that the parameters needed, so I
made it multiplicative instead of additive. This helped, but created a
car that was too oversensitive and didn't hold up well on turns. I found
that if I kept the ratio of P and D the same and just pulled them both
down, I eventually got a result that I liked.



### P
The P component was the most important part of my implementation. It was
what made the car steer towards the center of the track whenever possible.
The strength of the P constant made the car either more or less aggressive
when steering towards the center.
When the P value was too high, the car tended to swerve:

![P too high](screencasts/P_too_high.gif "P too high")

When it was too low, the other components effectively rendered the
controller useless:

![P too low](screencasts/P_too_low.gif "P too low")

### I
The I component is intended to help assuage any possible bias in the
car's steering. However, for this project I found it best just to set
it at zero. Setting it too high resulted in extreme oversensitivity:

![I too high](screencasts/I_too_high.gif "I too high")

### D
The D component is really important to smooth driving. The higher D is,
the smoother the car will drive. However, it will be less sensitive.

When D is set too high, the car isn't very sensitive and drives smoothly
at first but more dangerously when the P component causes the car to
swerve back to the center.

![D too high](screencasts/D_too_high.gif "D too high")

When D is too low, swerving starts almost instantly:

![D too low](screencasts/D_too_low.gif "D too low")






