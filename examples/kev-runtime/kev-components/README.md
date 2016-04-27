# Introduction

So you are willing to take the uncertain path of using Kevoree to tame the Internet of Things.
Then, we gladly welcome you in the beginning of your quest because. But we should advice you that the journey won't be easy, 
that there are untold histories when it comes to the low-level path, and they all will be revealed to you, and your life forever will change.
Be specially aware of the painful ways of Contiki and its infamous power to make you quit.

If you are still there, trembling but standing before the herculean task, we tell you:
'fear not, we are to turn on the light in the darkest night'

# Really, what is this about?

Ok, Ok, it is painful to implement this, so we are just trying to have some fun.
If you really want to know, Kevoree is a framework to execute distributed applications which is built on the models at runtime paradigm.

It is essentially a component model plus a models at runtime layer that provide a sort of both-ways reflection mechanism. In short, you can see
the architectural model of the running system, but you can also update such a model and see the changes reflected in the running system.

This git repository contains a lightweight implementation for Contiki-based embedded devices.

# How you use it?

The are a few artifacts in the system:
- We have a __firmware__ which includes Contiki and __the Kevoree Core Runtime__. You must deploy 
this firmware on each mote you want to reconfigure at runtime.
- We have models that describe distributed applications. Please go to Kevoree.org for further information.
- We have components types and other Kevoree Types that describe the business logic of your system.

To create a distributed system for the Internet of Things you usually have to follow a few steps after defining
the platform: i) define deploy units, ii) define components and other types withint deploy units, and iii)
define an system by creating a model to describe it. 
