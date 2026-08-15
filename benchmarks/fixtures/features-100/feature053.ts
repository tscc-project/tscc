function id53<T>(x: T): T { return x; }
class Base53<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child53 extends Base53<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E53 { A, B = 3, C, Label = "label" }
const x53: number = id53<number>(53);
const c53 = new Child53("n", x53);
console.log(c53.name, E53.A, E53[3]);
