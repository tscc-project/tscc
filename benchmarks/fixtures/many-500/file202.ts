interface Item202 { id: number; name: string; }
type Key202 = string | number;
export function make202(id: number, name: string): Item202 {
  const value: Item202 = {id, name};
  return value as Item202;
}
export const item202: Item202 = make202(202, "item-202");
