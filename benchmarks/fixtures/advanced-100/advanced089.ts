namespace N89 {
  export const base: number = 89;
  export function add(x: number): number { return base + x; }
}
function over89(x: number): number;
function over89(x: string): string;
function over89(x: number | string): number | string { return x; }
abstract class A89 { abstract f(x: number): number; }
class B89 extends A89 { override f(x: number): number { return x + 1; } }
enum E89 { A = 5 / 2, B, C = 2 ** 3, D }
const angle89 = <number>(89 + 1);
const check89 = {x: angle89} satisfies {x: number};
console.log(N89.add(1), over89(89), new B89().f(1), E89.B, E89.D, check89.x);
