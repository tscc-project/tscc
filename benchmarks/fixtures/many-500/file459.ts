interface Item459 { id: number; name: string; }
type Key459 = string | number;
export function make459(id: number, name: string): Item459 {
  const value: Item459 = {id, name};
  return value as Item459;
}
export const item459: Item459 = make459(459, "item-459");
