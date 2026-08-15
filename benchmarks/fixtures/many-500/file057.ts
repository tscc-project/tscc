interface Item57 { id: number; name: string; }
type Key57 = string | number;
export function make57(id: number, name: string): Item57 {
  const value: Item57 = {id, name};
  return value as Item57;
}
export const item57: Item57 = make57(57, "item-57");
