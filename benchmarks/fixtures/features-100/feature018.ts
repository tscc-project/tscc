function id18<T>(x: T): T { return x; }
class Base18<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child18 extends Base18<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E18 { A, B = 3, C, Label = "label" }
const x18: number = id18<number>(18);
const c18 = new Child18("n", x18);
console.log(c18.name, E18.A, E18[3]);
