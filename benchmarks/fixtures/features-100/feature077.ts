function id77<T>(x: T): T { return x; }
class Base77<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child77 extends Base77<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E77 { A, B = 3, C, Label = "label" }
const x77: number = id77<number>(77);
const c77 = new Child77("n", x77);
console.log(c77.name, E77.A, E77[3]);
