function id3<T>(x: T): T { return x; }
class Base3<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child3 extends Base3<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E3 { A, B = 3, C, Label = "label" }
const x3: number = id3<number>(3);
const c3 = new Child3("n", x3);
console.log(c3.name, E3.A, E3[3]);
