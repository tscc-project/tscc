interface Item126 { id: number; name: string; }
type Key126 = string | number;
export function make126(id: number, name: string): Item126 {
  const value: Item126 = {id, name};
  return value as Item126;
}
export const item126: Item126 = make126(126, "item-126");
