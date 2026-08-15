function id87<T>(x: T): T { return x; }
class Base87<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child87 extends Base87<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E87 { A, B = 3, C, Label = "label" }
const x87: number = id87<number>(87);
const c87 = new Child87("n", x87);
console.log(c87.name, E87.A, E87[3]);
