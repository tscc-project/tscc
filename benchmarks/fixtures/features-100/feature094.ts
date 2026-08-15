function id94<T>(x: T): T { return x; }
class Base94<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child94 extends Base94<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E94 { A, B = 3, C, Label = "label" }
const x94: number = id94<number>(94);
const c94 = new Child94("n", x94);
console.log(c94.name, E94.A, E94[3]);
