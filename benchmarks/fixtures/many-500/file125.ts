interface Item125 { id: number; name: string; }
type Key125 = string | number;
export function make125(id: number, name: string): Item125 {
  const value: Item125 = {id, name};
  return value as Item125;
}
export const item125: Item125 = make125(125, "item-125");
