interface Item141 { id: number; name: string; }
type Key141 = string | number;
export function make141(id: number, name: string): Item141 {
  const value: Item141 = {id, name};
  return value as Item141;
}
export const item141: Item141 = make141(141, "item-141");
