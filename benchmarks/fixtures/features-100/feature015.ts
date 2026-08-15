function id15<T>(x: T): T { return x; }
class Base15<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child15 extends Base15<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E15 { A, B = 3, C, Label = "label" }
const x15: number = id15<number>(15);
const c15 = new Child15("n", x15);
console.log(c15.name, E15.A, E15[3]);
