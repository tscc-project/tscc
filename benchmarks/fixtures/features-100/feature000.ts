function id0<T>(x: T): T { return x; }
class Base0<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child0 extends Base0<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E0 { A, B = 3, C, Label = "label" }
const x0: number = id0<number>(0);
const c0 = new Child0("n", x0);
console.log(c0.name, E0.A, E0[3]);
