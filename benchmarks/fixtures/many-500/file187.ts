interface Item187 { id: number; name: string; }
type Key187 = string | number;
export function make187(id: number, name: string): Item187 {
  const value: Item187 = {id, name};
  return value as Item187;
}
export const item187: Item187 = make187(187, "item-187");
