function id14<T>(x: T): T { return x; }
class Base14<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child14 extends Base14<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E14 { A, B = 3, C, Label = "label" }
const x14: number = id14<number>(14);
const c14 = new Child14("n", x14);
console.log(c14.name, E14.A, E14[3]);
