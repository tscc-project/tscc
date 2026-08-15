function id20<T>(x: T): T { return x; }
class Base20<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child20 extends Base20<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E20 { A, B = 3, C, Label = "label" }
const x20: number = id20<number>(20);
const c20 = new Child20("n", x20);
console.log(c20.name, E20.A, E20[3]);
