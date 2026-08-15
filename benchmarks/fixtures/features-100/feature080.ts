function id80<T>(x: T): T { return x; }
class Base80<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child80 extends Base80<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E80 { A, B = 3, C, Label = "label" }
const x80: number = id80<number>(80);
const c80 = new Child80("n", x80);
console.log(c80.name, E80.A, E80[3]);
