.. _Particle:

==================
Particle
==================

The particle widget renders dynamic visual effects on screen, such as fireworks, snow, bubbles, lightning, and more. The system includes a rich set of preset effects that developers can use directly to add vivid animations to their applications.

Feature Overview
----------------

The particle widget is built on a high-performance particle engine, providing a complete 2D particle system for dynamic visual effects on embedded devices.

Emission Shapes
~~~~~~~~~~~~~~~

Multiple geometric emission sources for flexible control over initial particle distribution:

.. list-table::
   :header-rows: 1
   :widths: 20 80

   * - Shape
     - Description
   * - Point
     - Emit from a single coordinate. Ideal for burst and jet effects.
   * - Line
     - Emit uniformly along a line segment. Ideal for rain, snow, and edge-based effects.
   * - Circle
     - Emit randomly within a circular area.
   * - Rect
     - Emit randomly within a rectangular area. Ideal for full-screen distribution.
   * - Ring
     - Emit within an annular region between inner and outer radii. Ideal for vortex and galaxy effects.

Trajectory Models
~~~~~~~~~~~~~~~~~

Three particle motion models:

.. list-table::
   :header-rows: 1
   :widths: 20 80

   * - Trajectory
     - Description
   * - Linear
     - Constant velocity motion with configurable damping.
   * - Gravity
     - Physics-based gravity simulation with acceleration, wind force, and damping.
   * - Orbit
     - Orbital motion around a specified center point.

Color System
~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 80

   * - Color Mode
     - Description
   * - Solid
     - Fixed single color.
   * - Random
     - Randomly selected from a palette of up to 8 colors.
   * - Gradient
     - Smooth transition from start color to end color over the particle's lifetime.
   * - Rainbow
     - Automatic hue cycling over the particle's lifetime.

Interaction Modes
~~~~~~~~~~~~~~~~~

Built-in touch interaction behaviors:

.. list-table::
   :header-rows: 1
   :widths: 25 75

   * - Behavior
     - Description
   * - Follow Touch
     - Emitter follows touch position in real time.
   * - Trail
     - Emits particles opposite to the touch movement direction, forming a trailing effect.
   * - Touch Feedback
     - Burst on press, continuous emission on drag, final burst on release.
   * - Pulse
     - Emission rate oscillates periodically.
   * - Breathe
     - Particle opacity oscillates periodically, creating a breathing glow.

Rendering Features
~~~~~~~~~~~~~~~~~~

- Three blend modes: Normal (standard alpha), Additive (glow), Multiply (darken)
- Four easing functions: Linear, Ease In, Ease Out, Ease In Out — controlling opacity and scale curves
- Independent per-particle scale, rotation, and opacity animation with velocity-aligned orientation
- Boundary behaviors: Kill, Reflect, Wrap

Extensibility
~~~~~~~~~~~~~

- Custom callbacks: 6 callback hooks (particle init, per-frame update, pre-render, death event, emitter start/stop) for fully custom behaviors
- Multi-emitter stacking: up to 16 independent effects running simultaneously on a single widget
- Configuration serialization: effect configs can be exported in binary format and loaded from memory or filesystem for resource management and dynamic switching

Usage
-----

Running the Particle Demo
~~~~~~~~~~~~~~~~~~~~~~~~~~

Enable the particle demo via ``menuconfig``. In the configuration UI, complete the following steps and save:

1. Go to ``Enable HoneyGUI Framework`` → ``HoneyGUI Feature Configuration``, enable ``Enable Particle System``
2. Go back, enter ``Select HoneyGUI Demo``, select ``Particle System Demo`` (this option depends on the previous step — the particle system must be enabled first)

.. note::

   ``Select HoneyGUI Demo`` is a single-choice menu. Selecting ``Particle System Demo`` will automatically deselect any previously chosen demo.

After launch, a selection menu (Launcher) is displayed. Tap an effect name to preview it, and tap the ``Back`` button at the top to return to the menu.

Using Preset Effects
~~~~~~~~~~~~~~~~~~~~

Each preset effect provides a standalone configuration function and demo entry point:

.. code-block:: c

   #include "effect_snow.h"

   /* Option 1: Initialize with default configuration */
   gui_particle_widget_t *widget = effect_snow_demo_init();

   /* Option 2: Get configuration and customize */
   particle_effect_config_t config;
   effect_snow_config(&config);

   /* Adjust parameters as needed, e.g. emission rate */
   config.emission.rate = 30.0f;

   /* Create particle widget and load configuration */
   gui_obj_t *root = gui_obj_get_root();
   gui_particle_widget_t *widget = gui_particle_widget_create(
       root, "my_snow", 0, 0, screen_w, screen_h, 256);
   gui_particle_widget_add_effect(widget, &config);

All preset effect headers and source files are located in ``example/widget/particle/``, named as ``effect_<name>.c/h``.


Preset Effects
--------------

The particle system provides the following preset effects, ready to use out of the box. Each effect has been carefully tuned for different application scenarios.

.. list-table::
   :header-rows: 1
   :widths: 18 35 47

   * - Effect
     - Description
     - Preview
   * - Galaxy
     - Rotating spiral galaxy with particles distributed along spiral arms, converging toward the center. Blue-purple outer gradient to white-pink core.
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/galaxy.mp4" type="video/mp4"></video></div>

   * - Firework
     - Timed bursts at random positions. Particles spread outward and fall with gravity. Multi-color random palette.
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/firework.mp4" type="video/mp4"></video></div>

   * - Trail
     - Continuously emits particles following touch position, forming a drag trail. Emits on press, stops on release.
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/trail.mp4" type="video/mp4"></video></div>

   * - Touch
     - Burst on tap, continuous emission on drag, final burst on release. Ideal for touch interaction feedback.
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/touch.mp4" type="video/mp4"></video></div>

   * - Snow
     - Snowflakes gently fall from the top of the screen with light wind drift and random rotation.
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/snow.mp4" type="video/mp4"></video></div>

   * - Bubble
     - Bubbles rise from the bottom, bounce off boundaries, and gradually expand.
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/bubble.mp4" type="video/mp4"></video></div>

   * - Custom
     - Demonstrates custom callback usage with sine-wave lateral perturbation and rainbow color cycling. Serves as a reference template for custom effects.
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/custom.mp4" type="video/mp4"></video></div>

   * - Tunnel
     - Particles radiate outward from screen center at high speed, simulating a warp-speed effect with perspective scaling.
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/tunnel.mp4" type="video/mp4"></video></div>

   * - Rocket
     - Dual-layer system: bright flame layer and gray smoke layer, simulating rocket thruster exhaust.
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/rocket.mp4" type="video/mp4"></video></div>

   * - Vortex
     - Particles spiral from the outer ring with rainbow colors. Rotation speeds up and size shrinks near the center.
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/vortex.mp4" type="video/mp4"></video></div>

   * - Lightning
     - Procedurally generated jagged bolt paths simulating real lightning discharge, with recursive branching and flicker fade-out.
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/lightning.mp4" type="video/mp4"></video></div>

   * - Fireflies
     - Glowing dots randomly distributed across the screen, slowly drifting with breathing opacity.
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/fireflies.mp4" type="video/mp4"></video></div>

   * - Ripple
     - Touch-triggered concentric rings expanding outward, simulating water ripples with secondary waves.
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/ripple.mp4" type="video/mp4"></video></div>

   * - Rain
     - Raindrops fall diagonally from the top with wind drift. Splash particles are generated on impact.
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/rain.mp4" type="video/mp4"></video></div>

   * - MagicCircle
     - Two magic circle images rotating in opposite directions, with orange-red sparks floating upward.
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/magic_circle.mp4" type="video/mp4"></video></div>

   * - LightBeam
     - Three colored laser beams reflecting off screen edges, with flash effects on bounce and additive color blending.
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/light_beam.mp4" type="video/mp4"></video></div>

.. note::

   The preset effect library is continuously expanding. We recommend regularly syncing the repository and running the Particle Demo to access the latest effect additions.

API
---

.. doxygenfile:: gui_particle_widget.h
