interface Item340 { id: number; name: string; }
type Key340 = string | number;
export function make340(id: number, name: string): Item340 {
  const value: Item340 = {id, name};
  return value as Item340;
}
export const item340: Item340 = make340(340, "item-340");
