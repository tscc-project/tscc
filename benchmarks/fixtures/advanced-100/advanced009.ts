namespace N9 {
  export const base: number = 9;
  export function add(x: number): number { return base + x; }
}
function over9(x: number): number;
function over9(x: string): string;
function over9(x: number | string): number | string { return x; }
abstract class A9 { abstract f(x: number): number; }
class B9 extends A9 { override f(x: number): number { return x + 1; } }
enum E9 { A = 5 / 2, B, C = 2 ** 3, D }
const angle9 = <number>(9 + 1);
const check9 = {x: angle9} satisfies {x: number};
console.log(N9.add(1), over9(9), new B9().f(1), E9.B, E9.D, check9.x);
