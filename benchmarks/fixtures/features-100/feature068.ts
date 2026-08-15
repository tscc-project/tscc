function id68<T>(x: T): T { return x; }
class Base68<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child68 extends Base68<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E68 { A, B = 3, C, Label = "label" }
const x68: number = id68<number>(68);
const c68 = new Child68("n", x68);
console.log(c68.name, E68.A, E68[3]);
