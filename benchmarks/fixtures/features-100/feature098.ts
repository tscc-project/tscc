function id98<T>(x: T): T { return x; }
class Base98<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child98 extends Base98<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E98 { A, B = 3, C, Label = "label" }
const x98: number = id98<number>(98);
const c98 = new Child98("n", x98);
console.log(c98.name, E98.A, E98[3]);
