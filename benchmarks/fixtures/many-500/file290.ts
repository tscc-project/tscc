interface Item290 { id: number; name: string; }
type Key290 = string | number;
export function make290(id: number, name: string): Item290 {
  const value: Item290 = {id, name};
  return value as Item290;
}
export const item290: Item290 = make290(290, "item-290");
