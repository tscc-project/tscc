function id90<T>(x: T): T { return x; }
class Base90<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child90 extends Base90<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E90 { A, B = 3, C, Label = "label" }
const x90: number = id90<number>(90);
const c90 = new Child90("n", x90);
console.log(c90.name, E90.A, E90[3]);
