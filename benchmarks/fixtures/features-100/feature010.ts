function id10<T>(x: T): T { return x; }
class Base10<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child10 extends Base10<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E10 { A, B = 3, C, Label = "label" }
const x10: number = id10<number>(10);
const c10 = new Child10("n", x10);
console.log(c10.name, E10.A, E10[3]);
