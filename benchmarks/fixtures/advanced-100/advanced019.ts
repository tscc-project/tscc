namespace N19 {
  export const base: number = 19;
  export function add(x: number): number { return base + x; }
}
function over19(x: number): number;
function over19(x: string): string;
function over19(x: number | string): number | string { return x; }
abstract class A19 { abstract f(x: number): number; }
class B19 extends A19 { override f(x: number): number { return x + 1; } }
enum E19 { A = 5 / 2, B, C = 2 ** 3, D }
const angle19 = <number>(19 + 1);
const check19 = {x: angle19} satisfies {x: number};
console.log(N19.add(1), over19(19), new B19().f(1), E19.B, E19.D, check19.x);
