# Decision 0002: world-day pacing

A world day is 24 world-hours and the default play pace maps one world day to
four hours of real-time play, equivalent to 6x world-time progression. Store
the world-day duration and its real-time duration separately in the committed
simulation configuration so the application can change the pace without
changing the deterministic fixed simulation step.

The headless CLI remains a batch runner and executes requested ticks as quickly
as possible. Real-time pacing belongs to an application loop that schedules
fixed simulation steps; wall-clock duration does not alter simulation state.
The fixed-step default was originally 1 ms and changed to one world-second
(decision 0003, task #62).

This decision fixes the default day-to-real-time mapping. It does not choose
calendar rules beyond a 24-hour day or define sleep, pausing, or offline
simulation behavior.
