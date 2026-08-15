namespace N18 {
  export const base: number = 18;
  export function add(x: number): number { return base + x; }
}
function over18(x: number): number;
function over18(x: string): string;
function over18(x: number | string): number | string { return x; }
abstract class A18 { abstract f(x: number): number; }
class B18 extends A18 { override f(x: number): number { return x + 1; } }
enum E18 { A = 5 / 2, B, C = 2 ** 3, D }
const angle18 = <number>(18 + 1);
const check18 = {x: angle18} satisfies {x: number};
console.log(N18.add(1), over18(18), new B18().f(1), E18.B, E18.D, check18.x);
