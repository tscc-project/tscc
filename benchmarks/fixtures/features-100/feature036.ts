function id36<T>(x: T): T { return x; }
class Base36<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child36 extends Base36<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E36 { A, B = 3, C, Label = "label" }
const x36: number = id36<number>(36);
const c36 = new Child36("n", x36);
console.log(c36.name, E36.A, E36[3]);
