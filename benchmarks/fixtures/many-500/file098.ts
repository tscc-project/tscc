interface Item98 { id: number; name: string; }
type Key98 = string | number;
export function make98(id: number, name: string): Item98 {
  const value: Item98 = {id, name};
  return value as Item98;
}
export const item98: Item98 = make98(98, "item-98");
