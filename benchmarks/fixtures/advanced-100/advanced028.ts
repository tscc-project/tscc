namespace N28 {
  export const base: number = 28;
  export function add(x: number): number { return base + x; }
}
function over28(x: number): number;
function over28(x: string): string;
function over28(x: number | string): number | string { return x; }
abstract class A28 { abstract f(x: number): number; }
class B28 extends A28 { override f(x: number): number { return x + 1; } }
enum E28 { A = 5 / 2, B, C = 2 ** 3, D }
const angle28 = <number>(28 + 1);
const check28 = {x: angle28} satisfies {x: number};
console.log(N28.add(1), over28(28), new B28().f(1), E28.B, E28.D, check28.x);
