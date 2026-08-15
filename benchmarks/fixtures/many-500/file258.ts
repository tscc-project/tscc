interface Item258 { id: number; name: string; }
type Key258 = string | number;
export function make258(id: number, name: string): Item258 {
  const value: Item258 = {id, name};
  return value as Item258;
}
export const item258: Item258 = make258(258, "item-258");
