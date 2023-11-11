# Evaluation strategy in boolean solids

Boolean solids are a special case in terms of solid evaluation. In
`nain4`, solids are not evaluated until either `.solid()`,
`.volume(...)` or `.place(...)` are called. This is also the case for
boolean solids. That is, a boolean solid will not exist until one of
those methods is called. Moreover, the mechanism to create boolean
solids is integrated in the builders of regular solids. For instance,
in order to subtract two boxes, the `sub` method is called on one of
them:

```c++
/**/ n4::box("box1").cube(2*m)
.sub(n4::box("box2").cube(1*m));
```

Note that, because we have not called `.solid()`, `.volume(...)` or
`.place(...)`, the boolean solid has not been created. But what
happened to `"box1"` and `"box2"`? We haven't called any of those
methods either, but they are eagerly evaluated when constructing
boolean solids.

In Geant4, both solids must exist prior to the creation of a boolean
solid. `nain4` allows the usage of Geant4 objects as well:

```c++
auto box2 = new G4Box("box2", 0.5*m, 0.5*m, 0.5*m);
/**/ n4::box("box1").cube(2*m)
.sub(box2);
```

In this case, `"box2"` exists beforehand and `"box1"` only exists
after `sub` is called.

## Chained operations

Following the reasoning above, when chaining boolean operations, the
nth step needs to evaluate the previous (n-1) steps. Here is an
example:

```c++
/*   */n4::box("box1").cube(...)  // box1 has not been "solidified" yet
  .add(n4::box("box2").cube(...)) // box1 and box 2 have been solidified
  .name("two-boxes")              // but the resulting boolean solid has not, yet
  .sub(n4::box("box3").cube(...)) // unless we perform another boolean operation
                                  // in which case, both two-boxes and box3 are solidified
  .name("two-minus-one-boxes")    // but not two-minus-one-boxes, yet
  .solid()                        // and of course, if we explicitly call `solid`
  ;                               // (or `volume` or `place`), it will be solidified
```


<!-- ## !!TODO!! -->

<!-- A summary of what needs to be explained: -->
<!-- + Calling any boolean method will eagerly evaluate both input solids, -->
<!--   but not the resuling boolean solid -->
<!-- + Applying the previous point to the concatenation of boolean -->
<!--   operations implies that for the nth call to a boolean method, the -->
<!--   previous (n-1) boolean operations must be evaluated. -->

<!-- Example: -->
<!--   ```c++ -->
<!--   /*   */n4::box("box1").cube(...)  // box1 has not been "solidified" yet -->
<!--     .add(n4::box("box2").cube(...)) // box1 and box 2 have been solidified -->
<!--     .name("two-boxes")              // but two-boxes has not, yet -->
<!--     .sub(n4::box("box3").cube(...)) // unless we perform another boolean operation -->
<!--                                     // in which case, both two-boxes and box3 are solidified -->
<!--     .name("two-minus-one-boxes")    // but not two-minus-one-boxes, yet -->
<!--     .solid()                        // and of course, if we explicitly call `solid` -->
<!--     ;                               // (or `volume` or `place`), it will be solidified -->
<!-- ``` -->
