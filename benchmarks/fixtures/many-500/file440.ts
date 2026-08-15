interface Item440 { id: number; name: string; }
type Key440 = string | number;
export function make440(id: number, name: string): Item440 {
  const value: Item440 = {id, name};
  return value as Item440;
}
export const item440: Item440 = make440(440, "item-440");
