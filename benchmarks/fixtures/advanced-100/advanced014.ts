namespace N14 {
  export const base: number = 14;
  export function add(x: number): number { return base + x; }
}
function over14(x: number): number;
function over14(x: string): string;
function over14(x: number | string): number | string { return x; }
abstract class A14 { abstract f(x: number): number; }
class B14 extends A14 { override f(x: number): number { return x + 1; } }
enum E14 { A = 5 / 2, B, C = 2 ** 3, D }
const angle14 = <number>(14 + 1);
const check14 = {x: angle14} satisfies {x: number};
console.log(N14.add(1), over14(14), new B14().f(1), E14.B, E14.D, check14.x);
