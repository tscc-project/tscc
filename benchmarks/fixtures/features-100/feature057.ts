function id57<T>(x: T): T { return x; }
class Base57<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child57 extends Base57<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E57 { A, B = 3, C, Label = "label" }
const x57: number = id57<number>(57);
const c57 = new Child57("n", x57);
console.log(c57.name, E57.A, E57[3]);
