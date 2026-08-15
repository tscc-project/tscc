interface Item445 { id: number; name: string; }
type Key445 = string | number;
export function make445(id: number, name: string): Item445 {
  const value: Item445 = {id, name};
  return value as Item445;
}
export const item445: Item445 = make445(445, "item-445");
