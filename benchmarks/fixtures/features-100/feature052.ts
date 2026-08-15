function id52<T>(x: T): T { return x; }
class Base52<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child52 extends Base52<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E52 { A, B = 3, C, Label = "label" }
const x52: number = id52<number>(52);
const c52 = new Child52("n", x52);
console.log(c52.name, E52.A, E52[3]);
