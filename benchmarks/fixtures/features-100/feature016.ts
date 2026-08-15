function id16<T>(x: T): T { return x; }
class Base16<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child16 extends Base16<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E16 { A, B = 3, C, Label = "label" }
const x16: number = id16<number>(16);
const c16 = new Child16("n", x16);
console.log(c16.name, E16.A, E16[3]);
