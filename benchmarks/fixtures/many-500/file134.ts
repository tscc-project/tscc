interface Item134 { id: number; name: string; }
type Key134 = string | number;
export function make134(id: number, name: string): Item134 {
  const value: Item134 = {id, name};
  return value as Item134;
}
export const item134: Item134 = make134(134, "item-134");
