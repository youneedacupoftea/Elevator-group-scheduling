# Elevator-group-scheduling
Design and implementation of elevator group control and scheduling algorithm.

## 1. Requirements
(1) Establish a comprehensive evaluation index for elevator group control.

(2) Design an algorithm and analyze its performance using data on different numbers of elevators and floor heights.

(3) Complete a demonstration of dynamic elevator group control.

## 2. Implementation
Since requests are not all sent at once, each external request needs to be assigned a suitable elevator to minimize waiting time and elevator energy consumption. When selecting a suitable elevator, the waiting time needs to be calculated, and the elevator with the shortest waiting time is the optimal elevator. When the elevator stops or moves upward, use formula (3); when the elevator moves downward, use formula (4).<img width="326" height="150" alt="屏幕截图 2025-11-08 060420" src="https://github.com/user-attachments/assets/d91964b1-d726-4c52-8a79-18882b6b1b32" />

• HC<sub>i</sub>: Elevator call from floor i;

• CF<sub>n</sub>: Current floor of elevator n;

• NF: Total number of floors (parameter set to 30);

• TF: Elevator travel time between adjacent floors;

• NS<sub>i</sub>: Number of times the elevator stops from CF<sub>n</sub> to HC<sub>i</sub>;

• PT: Negative time, equal to door opening time + door closing time + passenger entry time.

## 3. Result
<img width="501" height="508" alt="屏幕截图 2025-11-08 060849" src="https://github.com/user-attachments/assets/380978a0-3c85-4aa9-a909-fbbd914c58c4" />
