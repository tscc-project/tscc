function id88<T>(x: T): T { return x; }
class Base88<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child88 extends Base88<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E88 { A, B = 3, C, Label = "label" }
const x88: number = id88<number>(88);
const c88 = new Child88("n", x88);
console.log(c88.name, E88.A, E88[3]);
