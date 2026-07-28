# Work in progress!

The game is currently in the early, early development stages, though a comprehensive implementation plan is complete. The engine is mostly functional (excluding graphics right now, just primitive rendering) with a comprehensive test suite, application stack, and soon a GUI using Dear imGUI. I plan to add a developer tooling suite as well as hot-reload and an asset manager in the near-future.

My primary goal for this project is to develop my skills with C++, to follow stringent design strategies focused on performance and optimization, and to better understand rendering and graphics, as well as to try and make my own game for release further down the road.

I have made thorough use of my essentially feature-complete ECS system during testing and development and it currently utilizes a sparse/dense entity system with direct entity index references of all component parts by the systems. The systems also perform rapid look ups of valid entities in dense maps through a combination of careful selection of the "most-likely" smallest component pool, and via a component bitset signature held in the variable m_entityMask in each system file.

Updates currently run on both frame and client tick speeds, accommodating varied requirements of the systems, but I will be adding further tick cadences to the Scheduler class in order to update high-performance cost systems such as unit AI later in development. Parallelization is functional in one system right now, MovementTicks, but mostly just so I understood what was required of my systems to ensure easy refactoring for multi-threading later in many more systems. 

The game directory includes a functioning ship "baking" system that turns a (potentially very) large set of ship parts into a single vector of part structs. This will allow for gameplay operations such as directed damage via ray-casts and pseudo-realistic combat simulation. I am considering alternative data structures for this but it will be done in later optimization passes if needed.

If you are interested in any further details or potentially would like to contribute to this hobby project via recommendations for code improvements or features, I would love to hear from anyone that stumbles across it! My email address is on my profile.


# A small portion of the debugger output from the 90%+ coverage Catch2 test suite of the Engine directory.
<img width="1182" height="812" alt="image" src="https://github.com/user-attachments/assets/cdb2e995-fa51-4783-9306-9ca5be0bf9ca" />

