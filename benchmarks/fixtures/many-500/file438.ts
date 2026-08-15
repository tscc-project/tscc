interface Item438 { id: number; name: string; }
type Key438 = string | number;
export function make438(id: number, name: string): Item438 {
  const value: Item438 = {id, name};
  return value as Item438;
}
export const item438: Item438 = make438(438, "item-438");
