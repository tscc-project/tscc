interface Item292 { id: number; name: string; }
type Key292 = string | number;
export function make292(id: number, name: string): Item292 {
  const value: Item292 = {id, name};
  return value as Item292;
}
export const item292: Item292 = make292(292, "item-292");
