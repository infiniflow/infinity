.. _classes:

.. cpp:namespace:: nanobind

Classes
=======

The material below builds on the section on :ref:`binding custom types
<binding_types>` and reviews advanced scenarios involving object-oriented code.

Frequently used
---------------
Click on the following :cpp:class:`nb::class_\<..\>::def_* <class_>` members for
examples on how to bind various different kinds of methods, fields, etc.

.. list-table::
  :widths: 40 60
  :header-rows: 1

  * - Type
    - method
  * - Methods & constructors
    - :cpp:func:`.def() <class_::def>`
  * - Fields
    - :cpp:func:`.def_ro() <class_::def_ro>`,
      :cpp:func:`.def_rw() <class_::def_rw>`
  * - Properties
    - :cpp:func:`.def_prop_ro() <class_::def_prop_ro>`,
      :cpp:func:`.def_prop_rw() <class_::def_prop_rw>`
  * - Static methods
    - :cpp:func:`.def_static() <class_::def_static>`
  * - Static fields
    - :cpp:func:`.def_ro_static() <class_::def_ro_static>`,
      :cpp:func:`.def_rw_static() <class_::def_rw_static>`
  * - Static properties
    - :cpp:func:`.def_prop_ro_static() <class_::def_prop_ro_static>`,
      :cpp:func:`.def_prop_rw_static() <class_::def_prop_rw_static>`

.. _inheritance:

Subclasses
----------

Consider the following two data structures with an inheritance relationship:

.. code-block:: cpp

   struct Pet {
       std::string name;
   };

   struct Dog : Pet {
       std::string bark() const { return name + ": woof!"; }
   };

To indicate the inheritance relationship to nanobind, specify the C++ base
class as an extra template parameter of :cpp:class:`nb::class_\<..\> <class_>`:

.. code-block:: cpp
   :emphasize-lines: 8

   #include <nanobind/stl/string.h>

   NB_MODULE(my_ext, m) {
       nb::class_<Pet>(m, "Pet")
          .def(nb::init<const std::string &>())
          .def_rw("name", &Pet::name);

       nb::class_<Dog, Pet /* <- C++ parent type */>(m, "Dog")
           .def(nb::init<const std::string &>())
           .def("bark", &Dog::bark);
   }

Alternatively, you can also pass the type object as an ordinary parameter.

.. code-block:: cpp
   :emphasize-lines: 5

   auto pet = nb::class_<Pet>(m, "Pet")
      .def(nb::init<const std::string &>())
      .def_rw("name", &Pet::name);

   nb::class_<Dog>(m, "Dog", pet /* <- Parent type object */)
       .def(nb::init<const std::string &>())
       .def("bark", &Dog::bark);

Instances expose fields and methods of both types as expected:

.. code-block:: pycon

    >>> d = my_ext.Dog("Molly")
    >>> d.name
    'Molly'
    >>> d.bark()
    'Molly: woof!'

.. _automatic_downcasting:

Automatic downcasting
---------------------

nanobind obeys type signature when returning regular non-polymorphic C++ objects:
building on the :ref:`previous example <inheritance>`, consider the following
function that returns a ``Dog`` object as a ``Pet`` base pointer.

.. code-block:: cpp

   m.def("pet_store", []() { return (Pet *) new Dog{"Molly"}; });

nanobind cannot safely determine that this is in fact an instance of the
``Dog`` subclass. Consequently, only fields and methods of the base type remain
accessible:

.. code-block:: pycon

   >>> p = my_ext.pet_store()
   >>> type(p)
   <class 'my_ext.Pet'>
   >>> p.bark()
   AttributeError: 'Pet' object has no attribute 'bark'

In C++, a type is only considered `polymorphic
<https://en.wikipedia.org/wiki/Dynamic_dispatch>`_ if it (or one of its base
classes) has at least one *virtual function*. Let's add a virtual default
destructor to make ``Pet`` and its subtypes polymorphic.

.. code-block:: cpp

   struct Pet {
       virtual ~Pet() = default;
       std::string name;
   };

With this change, nanobind is able to inspect the returned C++ instance's
`virtual table <https://en.wikipedia.org/wiki/Virtual_method_table>`_ and infer
that it can be represented by a more specialized Python object of type
``my_ext.Dog``.

.. code-block:: pycon

   >>> p = my_ext.pet_store()
   >>> type(p)
   <class 'my_ext.Dog'>
   >>> p.bark()
   'Molly: woof!'

.. note::

   Automatic downcasting of polymorphic instances is only supported when the
   subtype has been registered using :cpp:class:`nb::class_\<..\> <class_>`.
   Otherwise, the return type listed in the function signature takes
   precedence.

.. _overloaded_methods:

Overloaded methods
------------------

Sometimes there are several overloaded C++ methods with the same name taking
different kinds of input arguments:

.. code-block:: cpp

    struct Pet {
        Pet(const std::string &name, int age) : name(name), age(age) { }

        void set(int age_) { age = age_; }
        void set(const std::string &name_) { name = name_; }

        std::string name;
        int age;
    };

Attempting to bind ``Pet::set`` will cause an error since the compiler does not
know which method the user intended to select. We can disambiguate by casting
them to function pointers. Binding multiple functions to the same Python name
automatically creates a chain of function overloads that will be tried in
sequence.

.. code-block:: cpp

   nb::class_<Pet>(m, "Pet")
      .def(nb::init<const std::string &, int>())
      .def("set", nb::overload_cast<int>(&Pet::set), "Set the pet's age")
      .def("set", nb::overload_cast<const std::string &>(&Pet::set), "Set the pet's name");

Here, :cpp:func:`nb::overload_cast <overload_cast>` only requires the parameter
types to be specified, and it deduces the return type.

.. note::

   In cases where a function overloads by ``const``-ness, an additional
   ``nb::const_`` parameter is needed to select the right overload, e.g.,
   ``nb::overload_cast<int>(&Pet::get, nb::const_)``.

   To define overloaded constructors, simply declare one after the other using
   the normal :cpp:class:`.def(nb::init\<...\>()) <init>` syntax.

The overload signatures are also visible in the method's docstring:

.. code-block:: pycon

   >>> help(my_ext.Pet)
   class Pet(builtins.object)
    |  Methods defined here:
    |
    |  __init__(...)
    |      __init__(self, arg0: str, arg1: int, /) -> None
    |
    |  set(...)
    |      set(self, arg: int, /) -> None
    |      set(self, arg: str, /) -> None
    |
    |      Overloaded function.
    |
    |      1. ``set(self, arg: int, /) -> None``
    |
    |      Set the pet's age
    |
    |      2. ``set(self, arg: str, /) -> None``
    |
    |      Set the pet's name

The format of the docstring with a leading overload list followed by a repeated
list with details is designed to be compatible with the `Sphinx
<https://www.sphinx-doc.org/en/master/>`_ documentation generator.

.. _enumerations_and_internal:

Enumerations and internal types
-------------------------------

Let's now suppose that the example class contains internal types like enumerations, e.g.:

.. code-block:: cpp

   struct Pet {
       enum Kind {
           Dog = 0,
           Cat
       };

       struct Attributes {
           float age = 0;
       };

       Pet(const std::string &name, Kind type) : name(name), type(type) { }

       std::string name;
       Kind type;
       Attributes attr;
   };

The binding code for this example looks as follows:

.. code-block:: cpp

   nb::class_<Pet> pet(m, "Pet");

   pet.def(nb::init<const std::string &, Pet::Kind>())
       .def_rw("name", &Pet::name)
       .def_rw("type", &Pet::type)
       .def_rw("attr", &Pet::attr);

   nb::enum_<Pet::Kind>(pet, "Kind")
       .value("Dog", Pet::Kind::Dog)
       .value("Cat", Pet::Kind::Cat)
       .export_values();

   nb::class_<Pet::Attributes>(pet, "Attributes")
       .def(nb::init<>())
       .def_rw("age", &Pet::Attributes::age);

To ensure that the nested types ``Kind`` and ``Attributes`` are created within
the scope of ``Pet``, the ``pet`` type object is passed as the ``scope``
argument of the subsequent :cpp:class:`nb::enum_\<T\> <enum_>` and
:cpp:class:`nb::class_\<T\> <class_>` binding declarations. The
:cpp:func:`.export_values() <enum_::export_values>` function exports the
enumeration entries into the parent scope, which should be skipped for newer
C++11-style strongly typed enumerations.

.. code-block:: pycon

   >>> from my_ext import Pet
   >>> p = Pet("Lucy", Pet.Cat)
   >>> p.attr.age = 3
   >>> p.type
   my_ext.Kind.Cat
   >>> p.type.__name__
   'Cat'
   >>> int(p.type)
   1

.. note::

   When the annotation :cpp:class:`nb::is_arithmetic() <is_arithmetic>` is
   passed to :cpp:class:`nb::enum_\<T\> <enum_>`, the resulting Python type
   will support arithmetic and bit-level operations like comparisons, and, or,
   xor, negation, etc.

   .. code-block:: cpp

       nb::enum_<Pet::Kind>(pet, "Kind", nb::is_arithmetic())
          ...

   By default, these are omitted.

.. _dynamic_attributes:

Dynamic attributes
------------------

Native Python classes can pick up new attributes dynamically:

.. code-block:: pycon

   >>> class Pet:
   ...     name = "Molly"
   ...
   >>> p = Pet()
   >>> p.name = "Charly"  # overwrite existing
   >>> p.age = 2  # dynamically add a new attribute

By default, classes exported from C++ do not support this and the only writable
attributes are the ones explicitly defined using :func:`class_::def_rw`
or :func:`class_::def_prop_rw`.

.. code-block:: cpp

   nb::class_<Pet>(m, "Pet")
       .def(nb::init<>())
       .def_rw("name", &Pet::name);

Trying to set any other attribute results in an error:

.. code-block:: pycon

   >>> p = my_ext.Pet()
   >>> p.name = "Charly"  # OK, attribute defined in C++
   >>> p.age = 2  # fail
   AttributeError: 'Pet' object has no attribute 'age'

To enable dynamic attributes for C++ classes, the :class:`nb::dynamic_attr
<dynamic_attr>` tag must be added to the :class:`nb::class_ <class_>`
constructor:

.. code-block:: cpp

   nb::class_<Pet>(m, "Pet", nb::dynamic_attr())
       .def(nb::init<>())
       .def_rw("name", &Pet::name);

Now everything works as expected:

.. code-block:: pycon

   >>> p = my_ext.Pet()
   >>> p.name = "Charly"  # OK, overwrite value in C++
   >>> p.age = 2  # OK, dynamically add a new attribute

Note that there is a small runtime cost for a class with dynamic attributes.
Not only because of the addition of an instance dictionary, but also because of
more expensive garbage collection tracking which must be activated to resolve
possible circular references. Native Python classes incur this same cost by
default, so this is not anything to worry about. By default, nanobind classes
are more efficient than native Python classes. Enabling dynamic attributes just
brings them on par.

.. _weak_refs:

Weak references
---------------

By default, nanobind instances cannot be referenced via Python's ``weakref``
class, and attempting to do so will raise an exception.

To support this, add the :class:`nb::is_weak_referenceable
<is_weak_referenceable>` tag to the :class:`nb::class_ <class_>` constructor.
Note that this will increase the size of every instance by ``sizeof(void*)``
due to the need to store a weak reference list.

.. code-block:: cpp

   nb::class_<Pet>(m, "Pet", nb::is_weak_referenceable());

.. _inheriting_in_python:

Extending C++ classes in Python
-------------------------------

Bound C++ types can be extended within Python, which is helpful to dynamically
extend compiled code with further fields and other functionality. Bind classes
with the :cpp:class:`is_final` annotation to forbid subclassing.

Consider the following example bindings of a ``Dog`` and ``DogHouse`` class.

.. code-block:: cpp

   #include <nanobind/stl/string.h>

   namespace nb = nanobind;

   struct Dog {
       std::string name;
       std::string bark() const { return name + ": woof!"; }
   };

   struct DogHouse {
       Dog dog;
   };

   NB_MODULE(my_ext, m) {
       nb::class_<Dog>(m, "Dog")
          .def(nb::init<const std::string &>())
          .def("bark", &Dog::bark)
          .def_rw("name", &Dog::name);

       nb::class_<DogHouse>(m, "DogHouse")
          .def(nb::init<Dog>())
          .def_rw("dog", &DogHouse::dog);
   }

The following Python snippet creates a new ``GuardDog`` type that extends
``Dog`` with an ``.alarm()`` method.

.. code-block:: pycon

   >>> import my_ext
   >>> class GuardDog(my_ext.Dog):
   ...     def alarm(self, count = 3):
   ...         for i in range(count):
   ...             print(self.bark())
   ...
   >>> gd = GuardDog("Max")
   >>> gd.alarm()
   Max: woof!
   Max: woof!
   Max: woof!

This Python subclass is best thought of as a "rich wrapper" around an existing
C++ base object. By default, that wrapper will disappear when nanobind makes a
copy or transfers ownership to C++.

.. code-block:: pycon

   >>> d = my_ext.DogHouse()
   >>> d.dog = gd
   >>> d.dog.alarm()
   AttributeError: 'Dog' object has no attribute 'alarm'

To preserve it, adopt a shared ownership model using :ref:`shared pointers
<shared_ptr>` or :ref:`intrusive reference counting <intrusive_intro>`. For
example, updating the code as follows fixes the problem:

.. code-block:: cpp

   #include <nanobind/stl/shared_ptr.h>

   struct DogHouse {
       std::shared_ptr<Dog> dog;
   };

.. code-block:: pycon

   >>> d = my_ext.DogHouse()
   >>> d.dog = gd
   >>> d.dog.alarm()
   Max: woof!
   Max: woof!
   Max: woof!

.. _trampolines:

Overriding virtual functions in Python
--------------------------------------

Building on the previous example on :ref:`inheriting C++ types in Python
<inheriting_in_python>`, let's investigate how a C++ *virtual function* can be
overridden in Python. In the code below, the virtual method ``bark()`` is
called by a global ``alarm()`` function (now written in C++).

.. code-block:: cpp
   :emphasize-lines: 6

   #include <iostream>

   struct Dog {
       std::string name;
       Dog(const std::string &name) : name(name) { }
       virtual std::string bark() const { return name + ": woof!"; }
   };

   void alarm(Dog *dog, size_t count = 3) {
       for (size_t i = 0; i < count; ++i)
           std::cout << dog->bark() << std::endl;
   }

Normally, the binding code would look as follows:

.. code-block:: cpp

   #include <nanobind/stl/string.h>

   namespace nb = nanobind;
   using namespace nb::literals;

   NB_MODULE(my_ext, m) {
       nb::class_<Dog>(m, "Dog")
          .def(nb::init<const std::string &>())
          .def("bark", &Dog::bark)
          .def_rw("name", &Dog::name);

       m.def("alarm", &alarm, "dog"_a, "count"_a = 3);
   }

However, this don't work as expected. We can subclass and override without
problems, but virtual function calls originating from C++ aren't being
propagated to Python:

.. code-block:: pycon

   >>> class ShihTzu(my_ext.Dog):
   ...     def bark(self):
   ...         return self.name + ": yip!"
   ...

   >>> dog = ShihTzu("Mr. Fluffles")

   >>> dog.bark()
   Mr. Fluffles: yip!

   >>> my_ext.alarm(dog)
   Mr. Fluffles: woof!     # <-- oops, alarm() is calling the base implementation
   Mr. Fluffles: woof!
   Mr. Fluffles: woof!

To fix this behavior, you must implement a *trampoline class*. A trampoline has
the sole purpose of capturing virtual function calls in C++ and forwarding them
to Python.

.. code-block:: cpp

   #include <nanobind/trampoline.h>

   struct PyDog : Dog {
       NB_TRAMPOLINE(Dog, 1);

       std::string bark() const override {
           NB_OVERRIDE(bark);
       }
   };

This involves an additional include directive and the line
:c:macro:`NB_TRAMPOLINE(Dog, 1) <NB_TRAMPOLINE>` to mark the class as a
trampoline for the ``Dog`` base type. The count (``1``) denotes to the total
number of virtual method slots that can be overridden within Python.

.. note::

   The number of virtual method slots is used to preallocate memory.
   Trampoline declarations with an insufficient size may eventually trigger a
   Python ``RuntimeError`` exception with a descriptive label, e.g.:

   .. code-block:: text

      nanobind::detail::get_trampoline('PyDog::bark()'): the trampoline ran out of
      slots (you will need to increase the value provided to the NB_TRAMPOLINE() macro)

The macro :c:macro:`NB_OVERRIDE(bark) <NB_OVERRIDE>` intercepts the virtual
function call, checks if a Python override exists, and forwards the call in
that case. If no override was found, it falls back to the base class
implementation. You will need to replicate this pattern for every method that
should support overriding in Python.

The macro accepts an variable argument list to pass additional parameters. For
example, suppose that the virtual function ``bark()`` had an additional ``int
volume`` parameter---in that case, the syntax would need to be adapted as follows:

.. code-block:: cpp

       std::string bark(int volume) const override {
           NB_OVERRIDE(bark, volume);
       }

The macro :c:macro:`NB_OVERRIDE_PURE() <NB_OVERRIDE_PURE>` should be used for
pure virtual functions, and :c:macro:`NB_OVERRIDE() <NB_OVERRIDE>` should be
used for functions which have a default implementation.  There are also two
alternate macros :c:macro:`NB_OVERRIDE_PURE_NAME() <NB_OVERRIDE_PURE_NAME>` and
:c:macro:`NB_OVERRIDE_NAME() <NB_OVERRIDE_NAME>` which take a string as first
argument to specify the name of function in Python. This is useful when the C++
and Python versions of the function have different names (e.g., ``operator+``
vs ``__add__``).

The binding code needs a tiny adaptation (highlighted) to inform nanobind of
the trampoline that will be used whenever Python code extends the C++ class.

.. code-block:: cpp

   nb::class_<Dog, PyDog /* <-- trampoline */>(m, "Dog")

If the :cpp:class:`nb::class_\<..\> <class_>` declaration also specifies a base
class, you may specify it and the trampoline in either order. Also, note that
binding declarations should be made against the actual class, not the
trampoline:

.. code-block:: cpp

    nb::class_<Dog, PyDog>(m, "Dog")
       .def(nb::init<const std::string &>())
       .def("bark", &PyDog::bark); /* <--- THIS IS WRONG, use &Dog::bark */

With the trampoline in place, our example works as expected:

.. code-block:: pycon

   >>> my_ext.alarm(dog)
   Mr. Fluffles: yip!
   Mr. Fluffles: yip!
   Mr. Fluffles: yip!

The following special case needs to be mentioned: you *may not* implement a
Python trampoline for a method that returns a reference or pointer to a
type requiring :ref:`type casting <type_casters>`. For example, attempting to
expose a hypothetical virtual method ``const std::string &get_name() const``
as follows

.. code-block:: cpp

       const std::string &get_name() const override {
           NB_OVERRIDE(get_name);
       }

will fail with a static assertion failure:

.. code-block:: text

   include/nanobind/nb_cast.h:352:13: error: static_assert failed due to requirement '...'
   "nanobind::cast(): cannot return a reference to a temporary."

This is not a fluke. The Python would return a ``str`` object that nanobind can
easily type-cast into a temporary ``std::string`` instance. However, when the
virtual function call returns on the C++ side, that temporary will already have
expired. There isn't a good solution to this problem, and nanobind therefore
simply refuses to do it. You will need to change your approach by either using
:ref:`bindings <bindings>` instead of :ref:`type casters <type_casters>` or
changing your virtual method interfaces to return by value.

.. _operator_overloading:

Operator overloading
--------------------

Suppose that we're given the following ``Vector2`` class with a vector addition
and scalar multiplication operation, all implemented using overloaded operators
in C++.

.. code-block:: cpp

   class Vector2 {
   public:
       Vector2(float x, float y) : x(x), y(y) { }

       Vector2 operator+(const Vector2 &v) const { return Vector2(x + v.x, y + v.y); }
       Vector2 operator*(float value) const { return Vector2(x * value, y * value); }
       Vector2 operator-() const { return Vector2(-x, -y); }
       Vector2& operator+=(const Vector2 &v) { x += v.x; y += v.y; return *this; }
       Vector2& operator*=(float v) { x *= v; y *= v; return *this; }

       friend Vector2 operator*(float f, const Vector2 &v) {
           return Vector2(f * v.x, f * v.y);
       }

       std::string to_string() const {
           return "[" + std::to_string(x) + ", " + std::to_string(y) + "]";
       }
   private:
       float x, y;
   };

The following snippet shows how the above operators can be conveniently exposed
to Python.

.. code-block:: cpp

    #include <nanobind/operators.h>

    NB_MODULE(my_ext, m) {
        nb::class_<Vector2>(m, "Vector2")
            .def(nb::init<float, float>())
            .def(nb::self + nb::self)
            .def(nb::self += nb::self)
            .def(nb::self *= float())
            .def(float() * nb::self)
            .def(nb::self * float())
            .def(-nb::self)
            .def("__repr__", &Vector2::to_string);
    }

Note that a line involving :cpp:var:`nb::self <self>` like

.. code-block:: cpp

   .def(nb::self * float())

is really just short hand notation for

.. code-block:: cpp

   .def("__mul__", [](const Vector2 &a, float b) {
       return a * b;
   }, nb::is_operator())

This can be useful for exposing additional operators that don't exist on the
C++ side, or to perform other types of customization. The
:cpp:class:`nb::is_operator() <is_operator>` flag marker is needed to inform
nanobind that this is an operator, which returns ``NotImplemented`` when
invoked with incompatible arguments rather than throwing a type error.

When binding *in-place* operators such as ``operator+=``, and when their
implementation is guaranteed to end with ``return *this``, it is recommended
that you set a return value policy of :cpp:enumerator:`rv_policy::none`, i.e.,

.. code-block:: cpp

   .def(nb::self += nb::self, nb::rv_policy::none)

Otherwise, the function binding will return a new copy of the object, which is
usually not desired.

Binding protected member functions
----------------------------------

It's normally not possible to expose ``protected`` member functions to Python:

.. code-block:: cpp

    class A {
    protected:
        int foo() const { return 42; }
    };

    nb::class_<A>(m, "A")
        .def("foo", &A::foo); // error: 'foo' is a protected member of 'A'

On one hand, this is good because non-``public`` members aren't meant to be
accessed from the outside. But we may want to make use of ``protected``
functions in derived Python classes.

The following pattern makes this possible:

.. code-block:: cpp

    class A {
    protected:
        int foo() const { return 42; }
    };

    class Publicist : public A { // helper type for exposing protected functions
    public:
        using A::foo; // inherited with different access modifier
    };

    nb::class_<A>(m, "A") // bind the primary class
        .def("foo", &Publicist::foo); // expose protected methods via the publicist

This works because ``&Publicist::foo`` is exactly the same function as
``&A::foo`` (same signature and address), just with a different access
modifier. The only purpose of the ``Publicist`` helper class is to make
the function name ``public``.

If the intent is to expose ``protected`` ``virtual`` functions which can be
overridden in Python, the publicist pattern can be combined with the previously
described trampoline:

.. code-block:: cpp

    class A {
    public:
        virtual ~A() = default;

    protected:
        virtual int foo() const { return 42; }
    };

    class Trampoline : public A {
    public:
        NB_TRAMPOLINE(A, 1);
        int foo() const override { NB_OVERRIDE(foo); }
    };

    class Publicist : public A {
    public:
        using A::foo;
    };

    nb::class_<A, Trampoline>(m, "A") // <-- `Trampoline` here
        .def("foo", &Publicist::foo); // <-- `Publicist` here, not `Trampoline`!

Binding classes with template parameters
----------------------------------------

nanobind can also wrap classes that have template parameters. Consider these classes:

.. code-block:: cpp

    struct Cat {};
    struct Dog {};

    template <typename PetType> struct PetHouse {
        PetHouse(PetType& pet);
        PetType& get();
    };

C++ templates may only be instantiated at compile time, so nanobind can only
wrap instantiated templated classes. You cannot wrap a non-instantiated template:

.. code-block:: cpp

    // BROKEN (this will not compile)
    nb::class_<PetHouse>(m, "PetHouse");
        .def("get", &PetHouse::get);

You must explicitly specify each template/type combination that you want to
wrap separately.

.. code-block:: cpp

    // ok
    nb::class_<PetHouse<Cat>>(m, "CatHouse")
        .def("get", &PetHouse<Cat>::get);

    // ok
    nb::class_<PetHouse<Dog>>(m, "DogHouse")
        .def("get", &PetHouse<Dog>::get);

If your class methods have template parameters you can wrap those as well,
but once again each instantiation must be explicitly specified:

.. code-block:: cpp

    typename <typename T> struct MyClass {
        template <typename V> T fn(V v);
    };

    nb::class_<MyClass<int>>(m, "MyClassT")
        .def("fn", &MyClass<int>::fn<std::string>);

.. _tag_based_polymorphism:

Tag-based polymorphism
----------------------

The section on :ref:`automatic downcasting <automatic_downcasting>` explained
how nanobind can infer the type of polymorphic C++ objects at runtime. It can
be desirable to extend this automatic downcasting behavior to non-polymorphic
classes, for example to support *tag-based polymorphism*. In this case,
instances expose a method or field to identify their type.

For example, consider the following class hierarchy where ``Pet::kind``
serves this purpose:

.. code-block:: cpp

   #include <nanobind/nanobind.h>

   namespace nb = nanobind;

   enum class PetKind { Cat, Dog };

   struct Pet { const PetKind kind; };
   struct Dog : Pet { Dog() : Pet{PetKind::Dog} { } };
   struct Cat : Pet { Cat() : Pet{PetKind::Cat} { } };

   namespace nb = nanobind;

   NB_MODULE(my_ext, m) {
       nb::class_<Pet>(m, "Pet");
       nb::class_<Dog>(m, "Dog");
       nb::class_<Cat>(m, "Cat");

       nb::enum_<PetKind>(m, "PetKind")
           .value("Cat", PetKind::Cat)
           .value("Dog", PetKind::Dog);

       m.def("make_pet", [](PetKind kind) -> Pet* {
           switch (kind) {
               case PetKind::Dog: return new Dog();
               case PetKind::Cat: return new Cat();
           }
       });
   }

This code initially doesn't work as expected (the ``make_pet`` function binding
always creates instances of the ``Pet`` base class).

.. code-block:: pycon

   >>> my_ext.make_pet(my_ext.PetKind.Cat)
   <my_ext.Pet object at 0x10305ee10>

   >>> my_ext.make_pet(my_ext.PetKind.Dog)
   <my_ext.Pet object at 0x10328e530>

To fix this, partially specialize the ``type_hook`` class to provide the
``type_hook<T>::get()`` method:

.. code-block:: cpp

   namespace nanobind::detail {
       template <> struct type_hook<Pet> {
           static const std::type_info *get(Pet *p) {
               if (p) {
                   switch (p->kind) {
                       case PetKind::Dog: return &typeid(Dog);
                       case PetKind::Cat: return &typeid(Cat);
                   }
               }
               return &typeid(Pet);
           }
       };
   } // namespace nanobind::detail

The method will be invoked whenever nanobind needs to convert a C++ pointer of
type ``T*`` to a Python object. It should inspect the instance and return a
pointer to a suitable RTTI record. With this override, downcasting works as
expected:

.. code-block:: pycon

    >>> my_ext.make_pet(my_ext.PetKind.Cat)
    <my_ext.Cat object at 0x104da6e10>

    >>> my_ext.make_pet(my_ext.PetKind.Dog)
    <my_ext.Dog object at 0x104da6ef0>

Binding unions
--------------

:cpp:class:`nb::class_\<..\> <class_>` can also be used to provide bindings
for `unions <https://en.cppreference.com/w/cpp/language/union>`__.
A basic and useless example:

.. code-block:: cpp

   union Example {
       int ival;
       double dval;

       std::string to_string(size_t active_idx) const {
           return active_idx == 1 ? std::to_string(dval) : std::to_string(ival);
       }
   };
   static_assert(sizeof(Example) == sizeof(double));

   nb::class_<Example>(m, "Example")
       .def_rw("ival", &Example::ival)
       .def_rw("dval", &Example::dval)
       .def("to_string", &Example::to_string);

.. code-block:: pycon

   >>> u = my_ext.Example()
   >>> u.ival = 42
   >>> u.to_string(0)
   '42'
   >>> u.dval = 1.25
   >>> u.to_string(1)
   '1.250000'

Direct binding of union variant members is only safe if all members of the
union are trivially copyable types (as in this example), but more complex
unions can also be supported by binding lambdas or member functions that
enforce the necessary invariants.

This is a low-level feature and should be used with care; even when all
members are trivially copyable, reading from a union member
other than the most recently written one produces undefined behavior
in C++. Unless you need to bind an existing API that uses union types,
you're probably better off using ``std::variant<..>``, which knows what
member is active and can thus enforce all the ncessary invariants for you.

Pickling
--------

To pickle and unpickle objects bound using nanobind, expose the
``__getstate__`` and ``__setstate__`` methods. They should return and retrieve
the internal instance state using representations that themselves support
pickling. The example below, e.g., does this using a tuple.

The ``__setstate__`` method should construct the object in-place analogous to
custom ``__init__``-style constructors.

.. code-block:: cpp

   #include <nanobind/stl/tuple.h>

   struct Pet {
        std::string name;
        int age;
        Pet(const std::string &name, int age) : name(name), age(age) { }
   };

   NB_MODULE(my_ext, m) {
       nb::class_<Pet>(m, "Pet")
          // ...
          .def("__getstate__", [](const Pet &pet) { return std::make_tuple(pet.name, pet.age); })
          .def("__setstate__", [](Pet &pet, const std::tuple<std::string, int> &state) {
                new (&pet) Pet(
                    std::get<0>(state),
                    std::get<1>(state)
                );
          });
    }

.. _custom_new:

Customizing Python object creation
----------------------------------

Sometimes you might need to bind a class that can't be constructed in the
usual way:

.. code-block:: cpp

   class Pet {
     private:
       Pet(/* ... */);
     public:
       static std::unique_ptr<Pet> make(std::string name, int age);
       void speak();
   };

You can use :cpp:func:`.def_static() <class_::def_static>` to
produce bindings that let you write ``Pet.make("Fido", 2)`` in Python,
just like you would write ``Pet::make("Fido", 2)`` in C++. But sometimes it's
nice to provide a more Pythonic interface than that, like ``Pet("Fido", 2)``.
To do that, nanobind lets you override ``__new__``.

Since this is a rarely-used feature in Python, let's recap. Object
initialization in Python occurs in two phases:

* the *constructor*, ``__new__``, allocates memory for the object;

* the *initializer*, ``__init__``, sets up the object's initial state.

So far, all the ways we've seen of binding C++ constructors
(:cpp:struct:`nb::init\<..\>() <init>`, ``.def("__init__", ...)``)
produce Python object *initializers*. nanobind augments these with its own
Python object constructor, which allocates a Python object that has
space in its memory layout for the C++ object to slot in.
The ``__init__`` method then fills in that space by calling a C++ constructor.

This split between ``__new__`` and ``__init__`` has a lot of benefits,
including a reduction in unnecessary allocations, but it does mean that
anything created from Python must be able to control where its
C++ innards are stored. Sometimes, as with the example of ``Pet`` above,
that's not feasible. In such cases, you can go down one level and override
``__new__`` directly:

.. code-block:: cpp

   nb::class_<Pet>(m, "Pet")
       .def(nb::new_(&Pet::make), "name"_a, "age"_a)
       .def("speak", &Pet::speak);

Passing :cpp:struct:`nb::new_ <new_>` to :cpp:func:`.def() <class_::def>`
here creates two magic methods on ``Pet``:

* A ``__new__`` that uses the given function to produce a new ``Pet``.
  It is converted to a Python object in the same way as the return value
  of any other function you might write bindings for. In particular,
  you can pass a :cpp:enum:`nb::rv_policy <rv_policy>` as an additional
  argument to :cpp:func:`.def() <class_::def>` to control how this conversion
  occurs.

* A ``__init__`` that takes the same arguments as ``__new__`` but
  performs no operation.  This is necessary because Python automatically
  calls ``__init__`` on the object returned by ``__new__`` in most cases.

You can provide a lambda as the argument of :cpp:struct:`nb::new_ <new_>`.
This is most useful when the lambda returns a pointer or smart pointer;
if it's returning a value, then ``.def("__init__", ...)`` will have better
performance. Additionally, you can chain multiple calls to
``.def(nb::new_(...))`` in order to create an overload set.
The following example demonstrates both of these capabilities:

.. code-block:: cpp

   nb::class_<Pet>(m, "Pet")
       .def(nb::new_([]() { return Pet::make(getRandomName(), 0); }))
       .def(nb::new_(&Pet::make), "name"_a, "age"_a)
       .def("speak", &Pet::speak);

If you need even more control, perhaps because you need to access the
type object that Python passes as the first argument of ``__new__``
(which :cpp:struct:`nb::new_ <new_>` discards), you can write a
``.def_static("__new__", ...)`` and matching ``.def("__init__", ...)``
yourself.

.. note::

   Unpickling an object of type ``Foo`` normally requires that
   ``Foo.__new__(Foo)`` produce something that ``__setstate__`` can be
   called on. Any custom :cpp:struct:`nb::new_ <new_>` methods will
   not satisfy this requirement, because they return a
   fully-constructed object. In order to maintain pickle
   compatibility, nanobind by default will add an additional
   ``__new__`` overload that takes no extra arguments and calls the nanobind
   built-in :cpp:func:`inst_alloc`. This won't make your class constructible
   with no arguments, because there's no corresponding ``__init__``; it
   just helps unpickling work. If your first :cpp:struct:`nb::new_ <new_>`
   method is one that takes no arguments, then nanobind won't add its own,
   and you'll have to deal with unpickling some other way.
