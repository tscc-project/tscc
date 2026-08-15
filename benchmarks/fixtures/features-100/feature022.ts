function id22<T>(x: T): T { return x; }
class Base22<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child22 extends Base22<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E22 { A, B = 3, C, Label = "label" }
const x22: number = id22<number>(22);
const c22 = new Child22("n", x22);
console.log(c22.name, E22.A, E22[3]);
