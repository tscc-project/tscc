namespace N22 {
  export const base: number = 22;
  export function add(x: number): number { return base + x; }
}
function over22(x: number): number;
function over22(x: string): string;
function over22(x: number | string): number | string { return x; }
abstract class A22 { abstract f(x: number): number; }
class B22 extends A22 { override f(x: number): number { return x + 1; } }
enum E22 { A = 5 / 2, B, C = 2 ** 3, D }
const angle22 = <number>(22 + 1);
const check22 = {x: angle22} satisfies {x: number};
console.log(N22.add(1), over22(22), new B22().f(1), E22.B, E22.D, check22.x);
