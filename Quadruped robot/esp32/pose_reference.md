# Pose Reference and Servo Calibration

The quadruped uses manually calibrated joint angles based on the physical assembly of the robot.
These values were experimentally determined by repeatedly testing standing stability, weight distribution and servo orientation after assembly.

Because the robot is assembled manually using hobby servos and 3D printed joints, the exact angles required for stable motion can vary between builds due to:

* servo horn alignment differences,
* mechanical tolerances,
* leg mounting offsets,
* center of gravity shifts,
* uneven servo neutral positions,
* frame asymmetry after tightening joints.

As a result, the angle values below should be treated as calibration references rather than universal values.

---

# Servo Channel Layout

Each PCA9685 controls 6 channels.

## Joint Mapping

| Channel | Joint Type   | Description                               |
| ------- | ------------ | ----------------------------------------- |
| 0       | Knee (Right) | Controls extension/folding of right leg   |
| 1       | Hip          | Controls forward/backward leg positioning |
| 2       | Shoulder     | Maintains upper leg orientation           |
| 13      | Shoulder     | Opposite side shoulder stabilization      |
| 14      | Hip          | Opposite side hip movement                |
| 15      | Knee (Left)  | Controls extension/folding of left leg    |

Two PCA9685 boards are used:

* Front PCA → front two legs
* Rear PCA → rear two legs

---

# Understanding the Joint Roles

## Shoulder Joint

The shoulder joints mainly maintain structural alignment and distribute weight across the upper body.

In the current implementation:

* shoulder movement is minimal,
* most dynamic motion comes from hip and knee joints,
* shoulders primarily stabilize the body posture.

Typical shoulder values remain near:

```text
110°
```

These values may vary slightly depending on:

* servo horn placement,
* body width,
* leg geometry.

---

## Hip Joint

The hip joint controls:

* forward leg movement,
* backward leg movement,
* body leaning during transitions.

Hip calibration was one of the most sensitive adjustments during tuning because even small offsets caused:

* lateral tilt,
* uneven weight distribution,
* instability during standing.

Higher or lower hip values directly affected:

* center of gravity,
* stepping smoothness,
* body symmetry.

---

## Knee Joint

The knee joint primarily controls:

* body height,
* crouching,
* standing extension.

This joint had the largest impact on:

* sitting posture,
* standing stability,
* servo load distribution.

Improper knee calibration caused:

* excessive servo strain,
* body imbalance,
* ground contact inconsistency.

Knee values were manually tuned until:

* all four legs shared weight evenly,
* the robot remained upright without excessive wobbling.

---

# Stand Pose

The following pose was experimentally determined to provide the most stable standing posture for the current assembly.

## Front PCA Angles

```text
{35, 110, 110, 110, 110, 80}
```

## Rear PCA Angles

```text
{65, 80, 110, 110, 140, 150}
```

### Characteristics

* Balanced center of gravity
* Minimal lateral tilt
* Reduced servo stress
* Stable idle posture

This pose acts as the default neutral position for:

* gesture responses,
* future gait development,
* stabilization testing.

---

# Sit Pose

The sit pose intentionally uses asymmetric angles to create a more natural crouched posture.

## Front PCA Angles

```text
{120, 60, 110, 110, 160, 0}
```

## Rear PCA Angles

```text
{180, 55, 110, 110, 155, 80}
```

### Characteristics

* Lower center of gravity
* Reduced front body elevation
* Stable resting posture
* Reduced servo load during idle

The asymmetry observed in some joints is due to:

* mirrored servo orientations,
* opposite motor rotation directions,
* assembly-specific compensation adjustments.

---

# Important Calibration Notes

## Mirrored Servo Rotation

Some servos are mounted facing opposite directions.
Because of this:

* increasing angle on one side may rotate clockwise,
* while the mirrored servo rotates counterclockwise.

This required independent calibration for:

* left and right legs,
* front and rear assemblies.

---

# Servo Neutral Offset

Not all servos center perfectly at 90°.
During assembly, each servo was manually tested and compensated to achieve:

* equal leg height,
* balanced posture,
* smoother transitions.

---

# Assembly Dependency

These angle values are highly dependent on:

* servo mounting position,
* horn alignment,
* frame geometry,
* tightening pressure on joints.

Even small mechanical differences between builds may require recalibration.

Therefore:

* these values should be used as a baseline,
* not as guaranteed universal joint positions.

---

# Future Improvements

Planned future upgrades include:

* inverse kinematics,
* dynamic gait generation,
* automated calibration,
* IMU-assisted adaptive balancing,
* closed-loop posture correction.

The current implementation uses manually tuned open-loop pose control.
