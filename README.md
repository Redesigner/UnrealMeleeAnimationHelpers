# UnrealMeleeAnimationHelpers
A plugin for unreal that sets up a basic melee hitbox system, with editor visualization.
Works so you can scrub through hitbox notifies in realtime, in the editor!

[MeleeAnimationHelpersPreview.webm](https://github.com/user-attachments/assets/f8874490-1662-4ec4-9bf7-ce3eed557666)

## Included Components
* Melee Helper Hitbox Component - Listener for animation notifies
* Hitbox Anim Notify State - Add hitboxes to animation
* Hitbox Anim Clear - Reset the list of overlapped actors and rechecks for overlaps
* Hitbox Attack Notify State - Alternative method for handling overlapping actors, to function better with blends and animation cancels

## How to use
* Add a Melee Helper Hitbox Component to your actor
* Insert animation notify states for each hitbox
* When your animation is done, either from the animation itself or code, call Hitbox Clear, to clear the list of overlapped actors from your Helper Hitbox Component

## Working with C++
If you're working with C++, and want to subclass the Anim Notify States, add "MeleeAnimationHelpers" to your PublicDependencyModuleNames in your Build.cs file. It should look something like this:
```
PublicDependencyModuleNames.AddRange(new string[]
{
  ...
  "MeleeAnimationHelpers"
});
```
